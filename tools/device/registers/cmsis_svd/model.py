#
# Copyright 2015 Paul Osborne <osbpau@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import json
import six

# Sentinel value for lookup where None might be a valid value
NOT_PRESENT = object()
TO_DICT_SKIP_KEYS = {"_register_arrays", "parent"}
REGISTER_PROPERTY_KEYS = {"size", "access", "protection", "reset_value", "reset_mask"}
LIST_TYPE_KEYS = {"register_arrays", "registers", "fields", "peripherals", "interrupts"}


def _check_type(value, expected_type):
    """Perform type checking on the provided value

    This is a helper that will raise ``TypeError`` if the provided value is
    not an instance of the provided type.  This method should be used sparingly
    but can be good for preventing problems earlier when you want to restrict
    duck typing to make the types of fields more obvious.

    If the value passed the type check it will be returned from the call.
    """
    if not isinstance(value, expected_type):
        raise TypeError(
            "Value {value!r} has unexpected type {actual_type!r}, expected {expected_type!r}".format(
                value=value,
                expected_type=expected_type,
                actual_type=type(value),
            )
        )
    return value


def _none_as_empty(v):
    if v is not None:
        for e in v:
            yield e


class SVDJSONEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, SVDElement):
            eldict = {}
            for k, v in six.iteritems(obj.__dict__):
                if k in TO_DICT_SKIP_KEYS:
                    continue
                if k.startswith("_"):
                    pubkey = k[1:]
                    eldict[pubkey] = getattr(obj, pubkey)
                else:
                    eldict[k] = v
            return eldict
        else:
            return json.JSONEncoder.default(self, obj)


class SVDElement(object):
    """Base class for all SVD Elements"""

    def __init__(self):
        self.parent = None

    def _lookup_possibly_derived_attribute(self, attr):
        derived_from = self.get_derived_from()

        # see if there is an attribute with the same name and leading underscore
        try:
            value_self = object.__getattribute__(self, "_{}".format(attr))
        except AttributeError:
            value_self = NOT_PRESENT

        # if not, then this is an attribute error
        if value_self is NOT_PRESENT:
            raise AttributeError("Requested missing key")

        # if there is a non-None value, that is what we want to use
        elif value_self is not None:
            return value_self  # if there is a non-None value, use it

        # if there is a derivedFrom, check there first
        elif derived_from is not None:
            derived_value = getattr(derived_from, "_{}".format(attr), NOT_PRESENT)
            if derived_value is not NOT_PRESENT:
                return derived_value

        # for some attributes, try to grab from parent
        if attr in REGISTER_PROPERTY_KEYS:
            value = getattr(self.parent, attr, value_self)
        else:
            value = value_self

        # if value is None and this is a list type, transform to empty list
        if value is None and attr in LIST_TYPE_KEYS:
            value = []

        return value

    def get_derived_from(self):
        pass  # override in children

    def to_dict(self):
        # This is a little convoluted but it works and ensures a
        # json-compatible dictionary representation (at the cost of
        # some computational overhead)
        encoder = SVDJSONEncoder()
        return json.loads(encoder.encode(self))


class SVDEnumeratedValue(SVDElement):
    def __init__(self, name, description, value, is_default):
        SVDElement.__init__(self)
        self.name = name
        self.description = description
        self.value = value
        self.is_default = is_default


class SVDField(SVDElement):
    def __init__(
        self,
        name,
        derived_from,
        description,
        bit_offset,
        bit_width,
        access,
        enumerated_values,
        modified_write_values,
        read_action,
    ):
        SVDElement.__init__(self)
        self.name = name
        self.derived_from = derived_from
        self.description = description
        self.bit_offset = bit_offset
        self.bit_width = bit_width
        self.access = access
        self.enumerated_values = enumerated_values
        self.modified_write_values = modified_write_values
        self.read_action = read_action

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    def get_derived_from(self):
        # TODO: add support for dot notation derivedFrom
        if self.derived_from is None:
            return None

        for field in self.parent.fields:
            if field.name == self.derived_from:
                return field

        raise KeyError("Unable to find derived_from: %r" % self.derived_from)

    @property
    def is_enumerated_type(self):
        """Return True if the field is an enumerated type"""
        return self.enumerated_values is not None

    @property
    def is_reserved(self):
        return self.name.lower() == "reserved"


class SVDRegisterArray(SVDElement):
    """Represent a register array in the tree"""

    def __init__(
        self,
        name,
        derived_from,
        description,
        address_offset,
        size,
        access,
        protection,
        reset_value,
        reset_mask,
        fields,
        display_name,
        alternate_group,
        modified_write_values,
        read_action,
        dim,
        dim_indices,
        dim_increment,
    ):
        SVDElement.__init__(self)

        # When deriving a register, it is mandatory to specify at least the name, the description,
        # and the addressOffset
        self.derived_from = derived_from
        self.name = name
        self.description = description
        self.address_offset = address_offset
        self.dim = dim
        self.dim_indices = dim_indices
        self.dim_increment = dim_increment

        self._read_action = read_action
        self._modified_write_values = modified_write_values
        self._display_name = display_name
        self._alternate_group = alternate_group
        self._size = size
        self._access = access
        self._protection = protection
        self._reset_value = reset_value
        self._reset_mask = reset_mask
        self._fields = fields

        # make parent association
        for field in self._fields:
            field.parent = self

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    @property
    def registers(self):
        for i in six.moves.range(self.dim):
            reg = SVDRegister(
                name=self.name % self.dim_indices[i],
                fields=self._fields,
                derived_from=self.derived_from,
                description=self.description,
                address_offset=self.address_offset + self.dim_increment * i,
                size=self._size,
                access=self._access,
                protection=self._protection,
                reset_value=self._reset_value,
                reset_mask=self._reset_mask,
                display_name=self._display_name,
                alternate_group=self._alternate_group,
                modified_write_values=self._modified_write_values,
                read_action=self._read_action,
            )
            reg.parent = self.parent
            yield reg

    def get_derived_from(self):
        # TODO: add support for dot notation derivedFrom
        if self.derived_from is None:
            return None

        for register in self.parent.registers:
            if register.name == self.derived_from:
                return register

        raise KeyError("Unable to find derived_from: %r" % self.derived_from)

    def is_reserved(self):
        return "reserved" in self.name.lower()


class SVDRegister(SVDElement):
    def __init__(
        self,
        name,
        derived_from,
        description,
        address_offset,
        size,
        access,
        protection,
        reset_value,
        reset_mask,
        fields,
        display_name,
        alternate_group,
        modified_write_values,
        read_action,
    ):
        SVDElement.__init__(self)

        # When deriving a register, it is mandatory to specify at least the name, the description,
        # and the addressOffset
        self.derived_from = derived_from
        self.name = name
        self.description = description
        self.address_offset = address_offset

        self._read_action = read_action
        self._modified_write_values = modified_write_values
        self._display_name = display_name
        self._alternate_group = alternate_group
        self._size = size
        self._access = access
        self._protection = protection
        self._reset_value = reset_value
        self._reset_mask = reset_mask
        self._fields = fields

        # make parent association
        for field in self._fields:
            field.parent = self

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    def get_derived_from(self):
        # TODO: add support for dot notation derivedFrom
        if self.derived_from is None:
            return None

        for register in self.parent.registers:
            if register.name == self.derived_from:
                return register

        raise KeyError("Unable to find derived_from: %r" % self.derived_from)

    def is_reserved(self):
        return "reserved" in self.name.lower()


class SVDRegisterCluster(SVDElement):
    """Represent a register cluster in the tree"""

    def __init__(
        self,
        name,
        derived_from,
        description,
        address_offset,
        size,
        alternate_cluster,
        header_struct_name,
        access,
        protection,
        reset_value,
        reset_mask,
        register,
        cluster,
    ):
        SVDElement.__init__(self)

        # When deriving a register, it is mandatory to specify at least the name, the description,
        # and the addressOffset
        self.derived_from = derived_from
        self.name = name
        self.description = description
        self.address_offset = address_offset

        self._alternate_cluster = alternate_cluster
        self._header_struct_name = header_struct_name
        self._size = size
        self._access = access
        self._protection = protection
        self._reset_value = reset_value
        self._reset_mask = reset_mask
        self._register = register
        self._cluster = cluster

        # make parent association
        for cluster in self._cluster:
            cluster.parent = self

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    def updated_register(self, reg, clu):
        new_reg = SVDRegister(
            name="{}_{}".format(clu.name, reg.name),
            fields=reg.fields,
            derived_from=reg.derived_from,
            description=reg.description,
            address_offset=clu.address_offset + reg.address_offset,
            size=reg.size,
            access=reg.access,
            protection=reg.protection,
            reset_value=reg.reset_value,
            reset_mask=reg.reset_mask,
            display_name=reg.display_name,
            alternate_group=reg.alternate_group,
            modified_write_values=reg.modified_write_values,
            read_action=reg.read_action,
        )
        new_reg.parent = self
        return new_reg

    @property
    def registers(self):
        for reg in self._register:
            yield self.updated_register(reg, self)
        for cluster in self._cluster:
            for reg in cluster.registers:
                yield self.updated_register(reg, self)

    def get_derived_from(self):
        # TODO: add support for dot notation derivedFrom
        if self.derived_from is None:
            return None

        for register in self.parent.registers:
            if register.name == self.derived_from:
                return register

        raise KeyError("Unable to find derived_from: %r" % self.derived_from)

    def is_reserved(self):
        return "reserved" in self.name.lower()


class SVDRegisterClusterArray(SVDElement):
    """Represent a register cluster in the tree"""

    def __init__(
        self,
        name,
        derived_from,
        description,
        address_offset,
        size,
        alternate_cluster,
        header_struct_name,
        dim,
        dim_indices,
        dim_increment,
        access,
        protection,
        reset_value,
        reset_mask,
        register,
        cluster,
    ):
        SVDElement.__init__(self)

        # When deriving a register, it is mandatory to specify at least the name, the description,
        # and the addressOffset
        self.derived_from = derived_from
        self.name = name
        self.description = description
        self.address_offset = address_offset
        self.dim = dim
        self.dim_indices = dim_indices
        self.dim_increment = dim_increment

        self._alternate_cluster = alternate_cluster
        self._header_struct_name = header_struct_name
        self._size = size
        self._access = access
        self._protection = protection
        self._reset_value = reset_value
        self._reset_mask = reset_mask
        self._register = register
        self._cluster = cluster

        # make parent association
        for register in self._register:
            register.parent = self
        for cluster in self._cluster:
            cluster.parent = self

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    def updated_register(self, reg, clu, i):
        new_reg = SVDRegister(
            name="{}_{}".format(clu.name % i, reg.name),
            fields=reg.fields,
            derived_from=reg.derived_from,
            description=reg.description,
            address_offset=clu.address_offset
            + reg.address_offset
            + i * clu.dim_increment,
            size=reg.size,
            access=reg.access,
            protection=reg.protection,
            reset_value=reg.reset_value,
            reset_mask=reg.reset_mask,
            display_name=reg.display_name,
            alternate_group=reg.alternate_group,
            modified_write_values=reg.modified_write_values,
            read_action=reg.read_action,
        )
        new_reg.parent = self
        return new_reg

    @property
    def registers(self):
        for i in six.moves.range(self.dim):
            for reg in self._register:
                yield self.updated_register(reg, self, i)
            for cluster in self._cluster:
                for reg in cluster.registers:
                    yield self.updated_register(reg, cluster, i)

    def get_derived_from(self):
        # TODO: add support for dot notation derivedFrom
        if self.derived_from is None:
            return None

        for register in self.parent.registers:
            if register.name == self.derived_from:
                return register

        raise KeyError("Unable to find derived_from: %r" % self.derived_from)

    def is_reserved(self):
        return "reserved" in self.name.lower()


class SVDAddressBlock(SVDElement):
    def __init__(self, offset, size, usage):
        SVDElement.__init__(self)
        self.offset = offset
        self.size = size
        self.usage = usage


class SVDInterrupt(SVDElement):
    def __init__(self, name, value, description):
        SVDElement.__init__(self)
        self.name = name
        self.value = _check_type(value, six.integer_types)
        self.description = description


class SVDPeripheral(SVDElement):
    def __init__(
        self,
        name,
        version,
        derived_from,
        description,
        prepend_to_name,
        base_address,
        address_blocks,
        interrupts,
        registers,
        register_arrays,
        size,
        access,
        protection,
        reset_value,
        reset_mask,
        group_name,
        append_to_name,
        disable_condition,
        clusters,
    ):
        SVDElement.__init__(self)

        # items with underscore are potentially derived
        self.name = name
        self._version = version
        self._derived_from = derived_from
        self._description = description
        self._prepend_to_name = prepend_to_name
        self._base_address = base_address
        self._address_blocks = address_blocks
        self._interrupts = interrupts
        self._registers = registers
        self._register_arrays = register_arrays
        self._size = size  # Defines the default bit-width of any register contained in the device (implicit inheritance).
        self._access = access  # Defines the default access rights for all registers.
        self._protection = (
            protection  # Defines extended access protection for all registers.
        )
        self._reset_value = (
            reset_value  # Defines the default value for all registers at RESET.
        )
        self._reset_mask = (
            reset_mask  # Identifies which register bits have a defined reset value.
        )
        self._group_name = group_name
        self._append_to_name = append_to_name
        self._disable_condition = disable_condition
        self._clusters = clusters

        # make parent association for complex node types
        for i in _none_as_empty(self._interrupts):
            i.parent = self
        for r in _none_as_empty(self._registers):
            r.parent = self
        for arr in _none_as_empty(self._register_arrays):
            arr.parent = self

    def __getattr__(self, attr):
        return self._lookup_possibly_derived_attribute(attr)

    @property
    def registers(self):
        regs = []
        for reg in self._lookup_possibly_derived_attribute("registers"):
            regs.append(reg)
        for arr in self._lookup_possibly_derived_attribute("register_arrays"):
            regs.extend(arr.registers)
        for cluster in self._lookup_possibly_derived_attribute("clusters"):
            regs.extend(cluster.registers)
        return regs

    def get_derived_from(self):
        if self._derived_from is None:
            return None

        # find the peripheral with this name in the tree
        try:
            return [p for p in self.parent.peripherals if p.name == self._derived_from][
                0
            ]
        except IndexError:
            return None


class SVDCpu(SVDElement):
    def __init__(
        self,
        name,
        revision,
        endian,
        mpu_present,
        fpu_present,
        fpu_dp,
        icache_present,
        dcache_present,
        itcm_present,
        dtcm_present,
        vtor_present,
        nvic_prio_bits,
        vendor_systick_config,
        device_num_interrupts,
        sau_num_regions,
        sau_regions_config,
    ):
        SVDElement.__init__(self)

        self.name = name
        self.revision = revision
        self.endian = endian
        self.mpu_present = mpu_present
        self.fpu_present = fpu_present
        self.fpu_dp = fpu_dp
        self.icache_present = (icache_present,)
        self.dcache_present = (dcache_present,)
        self.itcm_present = (itcm_present,)
        self.dtcm_present = (dtcm_present,)
        self.vtor_present = vtor_present
        self.nvic_prio_bits = nvic_prio_bits
        self.vendor_systick_config = vendor_systick_config
        self.device_num_interrupts = device_num_interrupts
        self.sau_num_regions = sau_num_regions
        self.sau_regions_config = sau_regions_config


class SVDDevice(SVDElement):
    def __init__(
        self,
        vendor,
        vendor_id,
        name,
        version,
        description,
        cpu,
        address_unit_bits,
        width,
        peripherals,
        size,
        access,
        protection,
        reset_value,
        reset_mask,
    ):
        SVDElement.__init__(self)

        self.vendor = vendor
        self.vendor_id = vendor_id
        self.name = name
        self.version = version
        self.description = description
        self.cpu = cpu
        self.address_unit_bits = _check_type(address_unit_bits, six.integer_types)
        self.width = _check_type(width, six.integer_types)
        self.peripherals = peripherals
        self.size = size  # Defines the default bit-width of any register contained in the device (implicit inheritance).
        self.access = access  # Defines the default access rights for all registers.
        self.protection = (
            protection  # Defines extended access protection for all registers.
        )
        self.reset_value = (
            reset_value  # Defines the default value for all registers at RESET.
        )
        self.reset_mask = (
            reset_mask  # Identifies which register bits have a defined reset value.
        )

        # set up parent relationship
        if self.cpu:
            self.cpu.parent = self

        for p in _none_as_empty(self.peripherals):
            p.parent = self
